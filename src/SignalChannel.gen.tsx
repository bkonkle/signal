/* TypeScript file generated from SignalChannel.re by genType. */
/* eslint-disable import/first */


// tslint:disable-next-line:no-var-requires
const Curry = require('bs-platform/lib/es6/curry.js');

// tslint:disable-next-line:no-var-requires
const SignalChannelBS = require('./SignalChannel.bs');

import {t as Signal_t} from './Signal.gen';

// tslint:disable-next-line:interface-over-type-literal
export type t<a> = { readonly signal: Signal_t<a> };

export const make: <a>(_1:a) => t<a> = SignalChannelBS.make;

export const channel: <T1>(_1:T1) => t<T1> = SignalChannelBS.channel;

export const send: <a>(_1:a, _2:t<a>) => void = function <a>(Arg1: any, Arg2: any) {
  const result = Curry._2(SignalChannelBS.send, Arg1, Arg2);
  return result
};

export const signal: <a>(_1:t<a>) => Signal_t<a> = SignalChannelBS.signal;
