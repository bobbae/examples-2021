import { TYPE_INCREMENT, TYPE_DECREMENT, TYPE_TRUE, TYPE_FALSE } from './actionTypes';

export const increment = () => ({
  type: TYPE_INCREMENT
});

export const decrement = () => ({
  type: TYPE_DECREMENT
});

export const setTrue = () => ({
  type: TYPE_TRUE
});

export const setFalse = () => ({
  type: TYPE_FALSE
});