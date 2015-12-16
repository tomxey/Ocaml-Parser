let id = function x -> x;;
(function i -> 2*i) 4;;
function n -> if n = 0 then id 3.14 else id 2.7;;
let madd = function a -> function b -> function c -> a + b + c;;
madd 1 2 3;;
let applier = function f -> function a -> f a;;
applier madd;;
applier madd 1;;
applier madd 1 2;;
applier madd 1 2 3;;
let itof = function i -> if i = 0 then 0.0 else 1.0;;
applier itof 1;;
let chooser = function c -> function t -> function f -> function a -> (if (c) then (t a) else (f a));;
let negative = function x -> x * (0-1);;
let positive = id;;
chooser (1=2) id negative 5;;

let zero = function n -> if n = 0 then 0 else zero (n-1);;
