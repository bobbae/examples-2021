const CheckStatus = (primiStatus) => {
  if (primiStatus === 'ack') {
    console.log('jeste ack');
  } else {
    console.log('nije ack');
  }
}

export default CheckStatus;