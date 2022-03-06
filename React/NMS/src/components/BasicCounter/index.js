import React from 'react';
import { connect } from 'react-redux';
import { increment, decrement } from '../../redux/actionCreators';

const CounterComponent = ({ count, handleIncrement, handleDecrement }) => {
  console.log(count)
  return (
    <div style={{margin: '50px', textAlign:'center'}}>
      <h1>Counter component</h1>
      <div>
        <button onClick={handleDecrement}>-</button>
        <span>{count}</span>
        <button onClick={handleIncrement}>+</button>
      </div>
    </div>
  )
}

const mapStateToProps = ({ count }) => ({
  count
});

const mapDispatchToProps = {
  handleDecrement: decrement,
  handleIncrement: increment
};

const BasicCounter = connect(
  mapStateToProps,
  mapDispatchToProps)(CounterComponent);

export default BasicCounter;