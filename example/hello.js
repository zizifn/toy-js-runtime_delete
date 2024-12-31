import { fib } from 'toyjsruntime:test';
import { test } from  './module.js';

console.log(`Hello, world! ${test}`);

console.log('fib method from toyjsruntime:test, and fib(5) is ', fib(5));
