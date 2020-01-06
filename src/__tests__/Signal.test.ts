import wait from 'waait'

import {
  Signal,
  constant,
  flatMap,
  merge,
  map,
  on,
  every,
  sampleOn,
  dropRepeats,
  foldp,
  filter,
  flatten,
} from '../Signal.gen'

export const tick = <A>(initial: number, interval: number, values: A[]) => {
  const vals = values.slice()

  const out = constant(vals.shift())

  if (vals.length) {
    setTimeout(function pop() {
      Signal.set(vals.shift(), out)

      if (vals.length) {
        setTimeout(pop, interval)
      }
    }, initial)
  }

  return out
}

export const getCalls = check => check.mock.calls.map(call => call[0])

describe('Signal', () => {
  describe('constant()', () => {
    it('yields a single value', () => {
      const check = jest.fn()

      flatMap(check, constant('lol'))

      expect(check).toHaveBeenCalledWith('lol')
    })

    it('yields the source constant when merging two constants', () => {
      const check = jest.fn()

      flatMap(check, merge(constant('bar'), constant('foo')))

      expect(check).toHaveBeenCalledWith('foo')
    })
  })

  describe('map()', () => {
    it('maps a function over a Signal', async () => {
      const check = jest.fn()

      const double = n => n * 2
      const ticker = tick(1, 1, [1, 2, 3])

      Signal.subscribe(check, map(double, ticker))

      await wait(50)

      expect(getCalls(check)).toEqual([2, 4, 6])
    })
  })

  describe('on()', () => {
    it('maps an effectful function over a signal', async () => {
      const check = jest.fn()

      const signalConverter = context => {
        context.state += 1
      }

      const ticker = tick(1, 1, [{state: 1}, {state: 2}, {state: 3}])

      Signal.subscribe(check, on(signalConverter, ticker))

      await wait(50)

      expect(getCalls(check)).toEqual([{state: 2}, {state: 3}, {state: 4}])
    })
  })

  describe('sampleOn()', () => {
    it('samples values from one Signal when another Signal changes', async () => {
      const check = jest.fn()

      const ticker = tick(10, 20, [1, 2, 3, 4, 5, 6])

      Signal.subscribe(check, sampleOn(ticker, every(40)))

      await wait(150)

      expect(getCalls(check)).toEqual([1, 3, 5, 6])
    })
  })

  describe('dropRepeats()', () => {
    it('only yields when value is != previous', async () => {
      const check = jest.fn()

      const ticker = tick(1, 1, [1, 1, 2, 2, 1, 3, 3])

      Signal.subscribe(check, dropRepeats(ticker))

      await wait(50)

      expect(getCalls(check)).toEqual([1, 2, 1, 3])
    })
  })

  describe('foldp()', () => {
    it('can sum up values', async () => {
      const check = jest.fn()

      const ticker = tick(1, 1, [1, 2, 3, 4, 5])

      Signal.subscribe(
        check,
        foldp((a, b) => a + b, 0, ticker)
      )

      await wait(50)

      expect(getCalls(check)).toEqual([1, 3, 6, 10, 15])
    })
  })

  describe('filter()', () => {
    it('filters values', async () => {
      const check = jest.fn()

      const ticker = tick(1, 1, [5, 3, 8, 4])

      Signal.subscribe(
        check,
        filter(n => n < 5, 0, ticker)
      )

      await wait(50)

      expect(getCalls(check)).toEqual([0, 3, 4])
    })
  })

  describe('flatten()', () => {
    it('flattens values', async () => {
      const check = jest.fn()

      const ticker = tick(10, 1, [[1, 2], [3, 4], [], [5, 6, 7]])

      Signal.subscribe(check, flatten(0, ticker))

      await wait(50)

      expect(getCalls(check)).toEqual([1, 2, 3, 4, 5, 6, 7])
    })

    it("uses the seed when it doesn't have new values yet", async () => {
      const check = jest.fn()

      const ticker = tick(10, 1, [[], [1, 2], [3, 4], [], [5, 6, 7]])

      Signal.subscribe(check, flatten(0, ticker))

      await wait(50)

      expect(getCalls(check)).toEqual([0, 1, 2, 3, 4, 5, 6, 7])
    })
  })
})
