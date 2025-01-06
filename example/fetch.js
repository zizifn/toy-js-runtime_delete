import { fetch } from 'toyjsruntime:net';

const resp = await fetch('https://jsonplaceholder.typicode.com/todos/1')

console.log('resp', resp);
