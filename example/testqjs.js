import {fib, callback} from 'toyjsruntime:test'

console.log(fib(10));

const test  =  callback(()=>{
    console.log("callback from log");
    return "callback";
})

console.log(test);