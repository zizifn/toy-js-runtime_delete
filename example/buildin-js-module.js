import {  testJSfunction, obj, testFetch} from "toyjsruntime:jsmodules";

console.log("testJSfunction", testJSfunction());
console.log("obj", obj);

testFetch().then(() => {console.log('xxxxxx')});