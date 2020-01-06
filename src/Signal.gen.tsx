/* TypeScript file generated from Signal.re by genType. */
/* eslint-disable import/first */


// tslint:disable-next-line:no-var-requires
const Curry = require('bs-platform/lib/es6/curry.js');

// tslint:disable-next-line:no-var-requires
const SignalBS = require('./Signal.bs');

import {list} from './ReasonPervasives.gen';

// tslint:disable-next-line:interface-over-type-literal
export type Signal_t<a> = { readonly value: [a]; readonly subscriptions: [list<(_1:a) => void>] };

export const Signal_make: <a>(_1:a) => Signal_t<a> = SignalBS.Signal.make;

export const Signal_get: <a>(_1:Signal_t<a>) => a = SignalBS.Signal.get;

export const Signal_set: <a>(_1:a, _2:Signal_t<a>) => void = function <a>(Arg1: any, Arg2: any) {
  const result = Curry._2(SignalBS.Signal.set, Arg1, Arg2);
  return result
};

export const Signal_subscribe: <a>(_1:((_1:a) => void), _2:Signal_t<a>) => void = function <a>(Arg1: any, Arg2: any) {
  const result = Curry._2(SignalBS.Signal.subscribe, Arg1, Arg2);
  return result
};

export const constant: <T1>(_1:T1) => Signal_t<T1> = SignalBS.constant;

export const run: (_1:Signal_t<((_1:void) => void)>) => void = SignalBS.run;

export const unwrap: <T1>(_1:Signal_t<((_1:void) => T1)>) => void = SignalBS.unwrap;

export const every: (_1:number) => Signal_t<number> = SignalBS.every;

export const map: <a,b>(_1:((_1:a) => b), _2:Signal_t<a>) => Signal_t<b> = function <a,b>(Arg1: any, Arg2: any) {
  const result = Curry._2(SignalBS.map, Arg1, Arg2);
  return result
};

export const flatMap: <a,b>(_1:((_1:a) => Signal_t<b>), _2:Signal_t<a>) => Signal_t<b> = function <a,b>(Arg1: any, Arg2: any) {
  const result = Curry._2(SignalBS.flatMap, Arg1, Arg2);
  return result
};

export const apply: <a,b>(_1:Signal_t<((_1:a) => b)>, _2:Signal_t<a>) => Signal_t<b> = function <a,b>(Arg1: any, Arg2: any) {
  const result = Curry._2(SignalBS.apply, Arg1, Arg2);
  return result
};

export const merge: <a>(_1:Signal_t<a>, _2:Signal_t<a>) => Signal_t<a> = function <a>(Arg1: any, Arg2: any) {
  const result = Curry._2(SignalBS.merge, Arg1, Arg2);
  return result
};

export const foldp: <a,b>(_1:((_1:a, _2:b) => b), _2:b, _3:Signal_t<a>) => Signal_t<b> = function <a,b>(Arg1: any, Arg2: any, Arg3: any) {
  const result = Curry._3(SignalBS.foldp, Arg1, Arg2, Arg3);
  return result
};

export const sampleOn: <a,b>(_1:Signal_t<b>, _2:Signal_t<a>) => Signal_t<b> = function <a,b>(Arg1: any, Arg2: any) {
  const result = Curry._2(SignalBS.sampleOn, Arg1, Arg2);
  return result
};

export const dropRepeats: <a>(_1:Signal_t<a>) => Signal_t<a> = SignalBS.dropRepeats;

export const filter: <a>(_1:((_1:a) => boolean), _2:a, _3:Signal_t<a>) => Signal_t<a> = function <a>(Arg1: any, Arg2: any, Arg3: any) {
  const result = Curry._3(SignalBS.filter, Arg1, Arg2, Arg3);
  return result
};

export const flatten: <b>(_1:b, _2:Signal_t<b[]>) => Signal_t<b> = function <b>(Arg1: any, Arg2: any) {
  const result = Curry._2(SignalBS.flatten, Arg1, Arg2);
  return result
};

export const on: <a>(_1:((_1:a) => void), _2:Signal_t<a>) => Signal_t<a> = function <a>(Arg1: any, Arg2: any) {
  const result = Curry._2(SignalBS.on, Arg1, Arg2);
  return result
};

export const delay: <a>(_1:number, _2:Signal_t<a>) => Signal_t<a> = function <a>(Arg1: any, Arg2: any) {
  const result = Curry._2(SignalBS.delay, Arg1, Arg2);
  return result
};

export const since: <a>(_1:number, _2:Signal_t<a>) => Signal_t<boolean> = function <a>(Arg1: any, Arg2: any) {
  const result = Curry._2(SignalBS.since, Arg1, Arg2);
  return result
};

export const Signal: {
  get: <a>(_1:Signal_t<a>) => a; 
  subscribe: <a>(_1:((_1:a) => void), _2:Signal_t<a>) => void; 
  set: <a>(_1:a, _2:Signal_t<a>) => void; 
  make: <a>(_1:a) => Signal_t<a>
} = SignalBS.Signal
