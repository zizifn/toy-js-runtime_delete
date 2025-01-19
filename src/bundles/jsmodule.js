import { fetch } from 'toyjsruntime:net';

function testJSfunction(){
    return 'test';
}

async function testFetch(){
    console.log("testFetch");
    const resp = fetch('https://jsonplaceholder.typicode.com/todos/1')
    console.log(`delay ${new Date().getTime()} ms`);
    const result = resp.then(value => {
        console.log('promise', value);
    }).catch(err =>{
        console.log('promise error', err);
    });

    await result;
}

const obj = {
    obj1: 'obj1',
}


export {
    testJSfunction,
    testFetch,
    obj
}