import wait from 'waait'

import {channel, send, signal} from '../SignalChannel.gen'
import {on, _subscribe} from '../Signal.gen'
import {tick, getCalls} from './Signal.test'

describe('SignalChannel', () => {
  describe('subscribe()', () => {
    it('yields when we send to the channel', async () => {
      const check = jest.fn()
      const chan = channel(1)
      const ticker = tick(1, 1, [2, 3, 4])

      on(value => send(value, chan), ticker)

      _subscribe(check, signal(chan))

      await wait(50)

      expect(getCalls(check)).toEqual([2, 3, 4])
    })
  })
})
