var ENVIRONMENT_IS_PTHREAD;
if (!ENVIRONMENT_IS_PTHREAD) ENVIRONMENT_IS_PTHREAD = false; // ENVIRONMENT_IS_PTHREAD=true will have been preset in pthread-main.js. Make it false in the main runtime thread.

console.log("Is pthread? " + ENVIRONMENT_IS_PTHREAD);

if(!ENVIRONMENT_IS_PTHREAD) {

var instanceToCreate;
