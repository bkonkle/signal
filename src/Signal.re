/**
 * Signal is a lightweight FRP-like Monad heavily inspired by the Elm Signal implementation. It
 * was ported from an original PureScript implementation created by
 * [Bodil Stokke](https://github.com/bodil/purescript-signal).
 *
 * @typeparam 'a - The Type of value the Signal yields
 */
[@genType]
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
let _get = (signal: t('a)) => signal.value^;

[@genType]
let _set = (value: 'a, signal: t('a)) => {
  signal.value := value;

  signal.subscriptions^ |> List.map(sub => sub(value)) |> ignore;
};

[@genType]
let _subscribe = (sub: 'a => unit, signal: t('a)) => {
  signal.subscriptions := [sub, ...signal.subscriptions^];

  sub(_get(signal));
};

/**
 * Create a signal with a constant value.
 */
[@genType]
let constant = make;

/**
 * Given a Signal of effects with no return value, run each effect as it comes in.
 */
[@genType]
let run = (signal: t(unit => unit)) =>
  signal |> _subscribe(value => value());

/**
 * Takes a signal of effects of 'a, and produces an effect which returns a signal which will take
 * each effect produced by the input signal, run it, and yield its returned value.
 */
[@genType]
let unwrap = (signal: t('a)) => {
  let run = _get(signal);
  let out = constant(run());
  let update = value => out |> _set(value());

  signal |> _subscribe(update);
};

/**
 * Creates a signal which yields the current time (according to now) every given number of
 * milliseconds.
 */
[@genType]
let every = (interval: int): t(float) => {
  let out = constant(Js.Date.now());

  Js.Global.setInterval(() => out |> _set(Js.Date.now()), interval) |> ignore;

  out;
};

/**
 * Apply a function to the Signal's value.
 */
[@genType]
let map: ('a => 'b, t('a)) => t('b) =
  (func, signal) => {
    let out = constant(func(_get(signal)));

    let produce = value => out |> _set(func(value));

    signal |> _subscribe(produce);

    out;
  };

/**
 * Bind a new operation to the Signal, transforming the value.
 */
[@genType]
let flatMap: ('a => t('b), t('a)) => t('b) =
  (func, signal) => func(_get(signal));

/**
 * Apply a function stored in another Signal to the value stored in this
 */
[@genType]
let apply: (t('a => 'b), t('a)) => t('b) =
  (source, signal) => {
    let func = _get(source);
    let out = constant(func(_get(signal)));

    let produce = _value => out |> _set(func(_get(signal)));

    source |> _subscribe(produce);
    signal |> _subscribe(produce);

    out;
  };

/**
 * Merge two signals, returning a new signal which will yield a value whenever either of the input
 * signals yield. Its initial value will be that of the first
 */
[@genType]
let merge: (t('a), t('a)) => t('a) =
  (source, signal) => {
    let out = constant(_get(signal));

    let produce = value => out |> _set(value);

    source |> _subscribe(produce);
    signal |> _subscribe(produce);

    out;
  };

/**
 * Creates a past dependent signal. The function argument takes the value of the input signal, and
 * the previous value of the output signal, to produce the new value of the output signal.
 */
[@genType]
let foldp: (('a, 'b) => 'b, 'b, t('a)) => t('b) =
  (func, seed, signal) => {
    let acc = ref(seed);
    let out = constant(acc^);

    let produce = value => {
      acc := func(value, acc^);

      out |> _set(acc^);
    };

    signal |> _subscribe(produce);

    out;
  };

/**
 * Creates a signal which yields the current value of the second signal every time the first
 * signal yields.
 */
[@genType]
let sampleOn: (t('b), t('a)) => t('b) =
  (source, signal) => {
    let out = constant(_get(source));

    let produce = _value => out |> _set(_get(source));

    signal |> _subscribe(produce);

    out;
  };

/**
 * Create a signal which only yields values which aren't equal to the previous value of the input
 * signal.
 */
[@genType]
let dropRepeats: t('a) => t('a) =
  signal => {
    let prev = ref(_get(signal));
    let out = constant(prev^);

    let produce = next =>
      if (prev^ !== next) {
        prev := next;

        out |> _set(prev^);
      };

    signal |> _subscribe(produce);

    out;
  };

/**
 * Takes a signal and filters out yielded values for which the provided predicate function returns
 * false.
 */
[@genType]
let filter: ('a => bool, 'a, t('a)) => t('a) =
  (func, a, signal) => {
    let value = _get(signal);
    let out = constant(func(value) ? value : a);

    let produce = value =>
      if (func(value)) {
        out |> _set(value);
      };

    signal |> _subscribe(produce);

    out;
  };

/**
 * Turn a signal of collections of items into a signal of each item inside each collection,
 * in order.
 */
[@genType]
let flatten: ('b, t('a)) => t('b) =
  (b, signal) => {
    let seed = ref(b);
    let first = ref(Some(Js.Array.copy(_get(signal))));

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
      Js.Array.forEach(item => out |> _set(item), items);

    let produce = value =>
      switch (first^) {
      | Some(f) =>
        feed(Js.Array.sliceFrom(1, f));

        first := None;
      | None => feed(value)
      };

    Js.Global.setTimeout(() => {signal |> _subscribe(produce)}, 0) |> ignore;

    out;
  };

/**
 * Runs side effects over the values of a Signal.
 */
[@genType]
let on: ('a => unit, t('a)) => t('a) =
  (func, signal) => {
    let produce = value => func(value);

    signal |> _subscribe(produce);

    signal;
  };

/**
 * Takes a signal and delays its yielded values by a given number of milliseconds.
 */
[@genType]
let delay: (int, t('a)) => t('a) =
  (time, signal) => {
    let out = constant(_get(signal));

    let first = ref(true);

    let produce = value =>
      if (first^) {
        first := false;
      } else {
        Js.Global.setTimeout(() => out |> _set(value), time) |> ignore;
      };

    signal |> _subscribe(produce);

    out;
  };

/**
 * Takes a signal and a time value, and creates a signal which yields True when the input signal
 * yields, then goes back to False after the given number of milliseconds have elapsed, unless the
 * input signal yields again in the interim.
 */
[@genType]
let since: (int, t('a)) => t(bool) =
  (time, signal) => {
    let out = constant(false);

    let first = ref(true);
    let timer = ref(None);

    let tick = () => {
      out |> _set(false);

      timer := None;
    };

    let produce = _value =>
      first^
        ? first := false
        : {
          switch (timer^) {
          | None => out |> _set(true)
          | Some(timer) => Js.Global.clearTimeout(timer)
          };

          timer := Some(Js.Global.setTimeout(tick, time));
        };

    signal |> _subscribe(produce);

    out;
  };
