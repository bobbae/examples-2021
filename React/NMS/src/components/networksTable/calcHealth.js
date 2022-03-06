import React from 'react';

const HealthBar = ({ primiHealth, primiStatus }) => {
  const health = Math.round(primiHealth / 20);
  const status = primiStatus;
  const check = () => {
    let sveee = [];
    if (status === 'ack') {
      sveee.push('ack', '😲', 8, 0);
      return sveee;
    } else if (health === 0) {
      sveee.push('red', '😰', 2, 6);
      return sveee;
    } else if (health === 1) {
      sveee.push('orange', '🙁', 4, 4);
      return sveee;
    } else if (health === 2 || health === 3) {
      sveee.push('yellow', '😐', 6, 2);
      return sveee;
    } else if (health === 4 || health === 5) {
      sveee.push('green', '🙂', 8, 0);
      return sveee;
    }
  }

  const color = check()[0];
  const smiley = check()[1];
  const fullRows = check()[2];
  const emptyRows = check()[3];

  const displayRows = (numOfRows) => {
    let counter = [];
    for (let i = 0; i < numOfRows; i++) {
      counter.push(numOfRows);
    }
    return counter;
  }

  return (
    <div className='ui-smiley-bar'>
      <div>
        <p>{smiley}</p>
      </div>
      {displayRows(fullRows).map(element => <div className={`ui-bar-` + color}></div>)}
      {displayRows(emptyRows).map(element => <div className={`ui-bar-` + color + `-empty`}></div>)}
    </div>
  )
}

export default HealthBar;







































































// function HealthFunction(){

//   let damjan = [];

//   let boban = data.map(el => el);

//   boban.forEach( e => damjan.push(e.health));
//   console.log(damjan);

//   let calcHealth = [];
//   damjan.forEach(e => calcHealth.push(e/20));
//   console.log(calcHealth)

//   boban.forEach(e => {
//     if (e.status === 'ack') {
//       console.log('jeste ack');
//     } else {
//       console.log('nije ack');
//     }
//   })
//   console.log('-------------------------');
// }