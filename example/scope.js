// let i=0;
// while(i < 5){
//     console.log(i);
//     i++;
// }

// for (let i = 0, getI = () => i; i < 3; i++) console.log(getI());
// for (let i = 0, getI = () => i; i < 3; i++, getI = () => i) console.log(getI());

// for (let i = 0, getI = () => i; i < 3; i++, getI = () => i) console.log(getI());
// for (let x = 1, getI= ()=> 
//     {
//         console.log(x);
//         return x;
//     }; x < 5; x++) {
//     setTimeout(() => {
//         getI();
        
//         console.log(x);
//     },1000)
  
// }

// function get(){
// let  i =100
//     return ()=>{
//         console.log(i);
//     }
// }

// get()();


for(let i= 0, getI = ()=> {
    console.log('init i', i);
}; i<5; i++){
    setTimeout(()=>{
        console.log(i);
        getI();
    }, 1000);
    //  i = i +1;
}