import * as ffi from '../src/modules/quickjs-ffi.js';

for (let k in ffi) {
    console.log(k, '=', ffi[k].toString());
}