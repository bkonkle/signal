/**
 * A Channel allows you to feed arbitrary values into a [[Signal]]. This is the simplest way to get
 * started with Signals.
 *
 * ```ts
 * const chan = channel('Hello, Bodil!')
 * const hello = chan.subscribe()
 *
 * // For each value sent to the Channel, transform it to uppercase and then log it.
 * hello.map(value => value.toUpperCase()).on(console.log)
 *
 * chan.send('This is great!')
 * ```
 */
[@genType]
type t('a) = {signal: Signal.t('a)};

/**
 * Creates a channel, which allows you to feed arbitrary values into a signal.
 */
[@genType]
let make = (value: 'a) => {signal: Signal.constant(value)};
[@genType]
let channel = make;

/**
 * Sends a value to a given channel.
 */
[@genType]
let send = (value: 'a, channel: t('a)) =>
  Signal._set(value, channel.signal);

/**
 * Returns the signal of the values sent to the channel.
 */
[@genType]
let signal = (channel: t('a)) => channel.signal;
