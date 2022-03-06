import { TYPE_INCREMENT, TYPE_DECREMENT } from '../actionTypes';

const initialState = {
  count: 0
};

const counterReducer = (state = initialState, action) => {
  switch (action.type) {
    case TYPE_INCREMENT:
      return {
        ...state,
        count: state.count + 1
      };
    case TYPE_DECREMENT:
      return {
        ...state,
        count: state.count - 1
      };
    default:
      return state;
  }
}

export default counterReducer;