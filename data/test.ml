let x = 0;;
let y = 1;;

let plus = function a -> function b -> a + b;;
let one = plus 1;;
let two = plus 2;;
let three = plus 3;;
one 1;;
two 1;;
three 1;;

let rec muladd = function a -> function b -> function n -> if n = 0 then 0 else a + b + (muladd a b (n-1));;
let rec muladd = function a -> function b -> function n -> if n = 0 then 0 else a + b + muladd a b (n-1);;
let x = if 1 + 1 = 2 + 0 then 2 + 2 else 2 + 3;;

type 'a list = End | Elem of ('a * (('a) list));;
type 'a list = End | Elem of 'a * 'a list;;

let curry = function f -> function x -> function y -> function z -> f(x,y,z);;
let uncurry = function f -> function t -> let (x,y,z) = t in f x y z;;
