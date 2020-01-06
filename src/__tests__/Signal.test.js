import wait from 'waait'

import {Signal, constant, flatMap, merge, map, on} from '../Signal.bs'

export const tick = (initial, interval, values) => {
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
        on(value => {
          value.state += 1
        }, context)
      }

      const ticker = tick(1, 1, [{state: 1}, {state: 2}, {state: 3}])

      Signal.subscribe(check, on(signalConverter, ticker))

      await wait(50)

      expect(getCalls(check)).toEqual([{state: 2}, {state: 3}, {state: 4}])
    })
  })
})
