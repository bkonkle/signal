type coordinatePair = {
  x: int,
  y: int,
};

type dimensionPair = {
  w: int,
  h: int,
};

type mouseButton =
  | Left
  | Middle
  | IE8Middle
  | Right;

type element;

let window: element = [%bs.raw "window"];

[@bs.send]
external addEventListener:
  (
    element,
    string,
    {
      .
      "keyCode": int,
      "button": int,
    } =>
    unit
  ) =>
  unit =
  "addEventListener";

/**
 * Creates a signal which will be `true` when the key matching the given key code is pressed, and
 * `false` when it's released.
 */
[@genType]
let keyPressed = (keyCode: int) => {
  let out = Signal.constant(false);

  addEventListener(window, "keydown", e =>
    if (e##keyCode === keyCode) {
      Signal._set(true, out);
    }
  );

  addEventListener(window, "keyup", e =>
    if (e##keyCode === keyCode) {
      Signal._set(false, out);
    }
  );

  out;
};

/**
 * Creates a signal which will be `true` when the given mouse button is pressed, and `false` when
 * it's released.
 */
// export const mouseButton = (button: number) => {
//   const out = constant(false)

//   window.addEventListener('mousedown', (e) => {
//     if (e.button === button) {
//       // @ts-ignore - internal exception
//       out.set(true)
//     }
//   })

//   window.addEventListener('mouseup', (e) => {
//     if (e.button === button) {
//       // @ts-ignore - internal exception
//       out.set(false)
//     }
//   })

//   return out
// }
let mouseButton = (button: int) => {
  let out = Signal.constant(false);

  addEventListener(window, "mousedown", e =>
    if (e##button === button) {
      Signal._set(true, out);
    }
  );

  addEventListener(window, "mouseup", e =>
    if (e##button === button) {
      Signal._set(false, out);
    }
  );

  out;
};

/**
 * Creates a signal which will be `true` when the given mouse button is pressed, and `false` when
 * it's released. Note: in IE8 and earlier you need to use MouseIE8MiddleButton if you want to query
 * the middle button.
 */
let mouseButtonPressed = (button: mouseButton) =>
  switch (button) {
  | Left => mouseButton(0)
  | Right => mouseButton(2)
  | Middle => mouseButton(2)
  | IE8Middle => mouseButton(4)
  };

/**
 * A signal containing the current state of the touch device.
 */;
// export const touch = () => {
//   const initial: Touch[] = []
//   const out = constant(initial)

//   const report = (e: TouchEvent) => {
//     const touches = []
//     const l = e.touches.length

//     for (let i = 0; i < l; i = i + 1) {
//       const t = e.touches.item(i)
//       if (t) {
//         touches.push(t)
//       }
//     }

//     // @ts-ignore - internal exception
//     out.set(touches)
//   }

//   window.addEventListener('touchstart', report)
//   window.addEventListener('touchend', report)
//   window.addEventListener('touchmove', report)
//   window.addEventListener('touchcancel', report)

//   return out
// }
