import {EventEmitter} from 'node:events';

class MyEmitter extends EventEmitter {
  constructor() {
    super();

    // use nextTick to emit the event once a handler is assigned
    Promise.resolve().then(() => {
      this.emit('promiseevent');
    });
    process.nextTick(() => {
      this.emit('nexttickevent');
    });
    setTimeout(() => {
      this.emit('timeoutevent');
    }, 0);
    setImmediate(() => {
      this.emit('immediateevent');
    });
  }
}

const myEmitter = new MyEmitter();
myEmitter.on('promiseevent', (VALUE) => {
  console.log('an promiseevent occurred!');
});
myEmitter.on('nexttickevent', (VALUE) => {
  console.log('an nexttickevent occurred!');
});
myEmitter.on('timeoutevent', (VALUE) => {
  console.log('an timeoutevent occurred!');
});
myEmitter.on('immediateevent', (VALUE) => {
  console.log('an immediateevent occurred!');
});
