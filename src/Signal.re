/**
 * Signal is a lightweight FRP-like Monad heavily inspired by the Elm Signal implementation. It
 * was ported from an original PureScript implementation created by
 * [Bodil Stokke](https://github.com/bodil/purescript-signal).
 *
 * @typeparam 'a - The Type of value the Signal yields
 */
module Signal = {
  type t('a) = {
    value: ref('a),
    subscriptions: ref(list('a => unit)),
  };

  [@genType]
  let make: 'a => t('a) =
    initial => {
      {value: ref(initial), subscriptions: ref([])};
    };

  [@genType]
  let get = (signal: t('a)) => signal.value^;

  [@genType]
  let set = (value: 'a, signal: t('a)) => {
    signal.value := value;

    signal.subscriptions^ |> List.map(sub => sub(value)) |> ignore;
  };

  [@genType]
  let subscribe = (sub: 'a => unit, signal: t('a)) => {
    signal.subscriptions := [sub, ...signal.subscriptions^];

    sub(get(signal));
  };
};

/**
 * Re-export the inner Signal type.
 */
type t('a) = Signal.t('a);

/**
 * Create a signal with a constant value.
 */
[@genType]
let constant = Signal.make;

/**
 * Given a Signal of effects with no return value, run each effect as it comes in.
 */
[@genType]
let run = (signal: Signal.t(unit => unit)) =>
  signal |> Signal.subscribe(value => value());

/**
 * Takes a signal of effects of 'a, and produces an effect which returns a signal which will take
 * each effect produced by the input signal, run it, and yield its returned value.
 */
[@genType]
let unwrap = (signal: Signal.t('a)) => {
  let run = Signal.get(signal);
  let out = constant(run());
  let update = value => out |> Signal.set(value());

  signal |> Signal.subscribe(update);
};

/**
 * Creates a signal which yields the current time (according to now) every given number of
 * milliseconds.
 */
[@genType]
let every = (interval: int): Signal.t(float) => {
  let out = constant(Js.Date.now());

  Js.Global.setInterval(() => out |> Signal.set(Js.Date.now()), interval)
  |> ignore;

  out;
};

/**
 * Apply a function to the Signal's value.
 */
[@genType]
let map: ('a => 'b, Signal.t('a)) => Signal.t('b) =
  (func, signal) => {
    let out = constant(func(Signal.get(signal)));

    let produce = value => out |> Signal.set(func(value));

    signal |> Signal.subscribe(produce);

    out;
  };

/**
 * Bind a new operation to the Signal, transforming the value.
 */
[@genType]
let flatMap: ('a => Signal.t('b), Signal.t('a)) => Signal.t('b) =
  (func, signal) => func(Signal.get(signal));

/**
 * Apply a function stored in another Signal to the value stored in this Signal.
 */
[@genType]
let apply: (Signal.t('a => 'b), Signal.t('a)) => Signal.t('b) =
  (source, signal) => {
    let func = Signal.get(source);
    let out = constant(func(Signal.get(signal)));

    let produce = _value => out |> Signal.set(func(Signal.get(signal)));

    source |> Signal.subscribe(produce);
    signal |> Signal.subscribe(produce);

    out;
  };

/**
 * Merge two signals, returning a new signal which will yield a value whenever either of the input
 * signals yield. Its initial value will be that of the first signal.
 */
[@genType]
let merge: (Signal.t('a), Signal.t('a)) => Signal.t('a) =
  (source, signal) => {
    let out = constant(Signal.get(signal));

    let produce = value => out |> Signal.set(value);

    source |> Signal.subscribe(produce);
    signal |> Signal.subscribe(produce);

    out;
  };

/**
 * Creates a past dependent signal. The function argument takes the value of the input signal, and
 * the previous value of the output signal, to produce the new value of the output signal.
 */
[@genType]
let foldp: (('a, 'b) => 'b, 'b, Signal.t('a)) => Signal.t('b) =
  (func, seed, signal) => {
    let acc = ref(seed);
    let out = constant(acc^);

    let produce = value => {
      acc := func(value, acc^);

      out |> Signal.set(acc^);
    };

    signal |> Signal.subscribe(produce);

    out;
  };

/**
 * Creates a signal which yields the current value of the second signal every time the first
 * signal yields.
 */
[@genType]
let sampleOn: (Signal.t('b), Signal.t('a)) => Signal.t('b) =
  (source, signal) => {
    let out = constant(Signal.get(source));

    let produce = _value => out |> Signal.set(Signal.get(source));

    signal |> Signal.subscribe(produce);

    out;
  };

/**
 * Create a signal which only yields values which aren't equal to the previous value of the input
 * signal.
 */
[@genType]
let dropRepeats: Signal.t('a) => Signal.t('a) =
  signal => {
    let prev = ref(Signal.get(signal));
    let out = constant(prev^);

    let produce = next =>
      if (prev^ !== next) {
        prev := next;

        out |> Signal.set(prev^);
      };

    signal |> Signal.subscribe(produce);

    out;
  };

/**
 * Takes a signal and filters out yielded values for which the provided predicate function returns
 * false.
 */
[@genType]
let filter: ('a => bool, 'a, Signal.t('a)) => Signal.t('a) =
  (func, a, signal) => {
    let value = Signal.get(signal);
    let out = constant(func(value) ? value : a);

    let produce = value =>
      if (func(value)) {
        out |> Signal.set(value);
      };

    signal |> Signal.subscribe(produce);

    out;
  };

/**
 * Turn a signal of collections of items into a signal of each item inside each collection,
 * in order.
 */
[@genType]
let flatten: ('b, Signal.t('a)) => Signal.t('b) =
  (b, signal) => {
    let seed = ref(b);
    let first = ref(Some(Js.Array.copy(Signal.get(signal))));

    switch (first^) {
    | Some(f) =>
      if (Js.Array.length(f) > 0) {
        seed := f[0];
      } else {
        first := None;
      }
    | None => ()
    };

    let out = constant(seed^);

    let feed = (items: Js.Array.t('b)) =>
      Js.Array.forEach(item => out |> Signal.set(item), items);

    let produce = value =>
      switch (first^) {
      | Some(f) =>
        feed(Js.Array.sliceFrom(1, f));

        first := None;
      | None => feed(value)
      };

    Js.Global.setTimeout(() => {signal |> Signal.subscribe(produce)}, 0)
    |> ignore;

    out;
  };

/**
 * Runs side effects over the values of a Signal.
 */
[@genType]
let on: ('a => unit, Signal.t('a)) => Signal.t('a) =
  (func, signal) => {
    let produce = value => func(value);

    signal |> Signal.subscribe(produce);

    signal;
  };

/**
 * Takes a signal and delays its yielded values by a given number of milliseconds.
 */
[@genType]
let delay: (int, Signal.t('a)) => Signal.t('a) =
  (time, signal) => {
    let out = constant(Signal.get(signal));

    let first = ref(true);

    let produce = value =>
      if (first^) {
        first := false;
      } else {
        Js.Global.setTimeout(() => out |> Signal.set(value), time) |> ignore;
      };

    signal |> Signal.subscribe(produce);

    out;
  };

/**
 * Takes a signal and a time value, and creates a signal which yields True when the input signal
 * yields, then goes back to False after the given number of milliseconds have elapsed, unless the
 * input signal yields again in the interim.
 */
[@genType]
let since: (int, Signal.t('a)) => Signal.t(bool) =
  (time, signal) => {
    let out = constant(false);

    let first = ref(true);
    let timer = ref(None);

    let tick = () => {
      out |> Signal.set(false);

      timer := None;
    };

    let produce = _value =>
      first^
        ? first := false
        : {
          switch (timer^) {
          | None => out |> Signal.set(true)
          | Some(timer) => Js.Global.clearTimeout(timer)
          };

          timer := Some(Js.Global.setTimeout(tick, time));
        };

    signal |> Signal.subscribe(produce);

    out;
  };
