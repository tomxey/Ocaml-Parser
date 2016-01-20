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

type 'a list = End | Elem of 'a * 'a list;;
let rec map = function f -> function l -> match l with End -> End | Elem(x,xs) -> Elem((f x), map f xs) ;;

type 'a list = End | Elem of 'a * 'a list;;
type 'a list = End | Elem of 'a * ('a list);;
match Elem(1,End) with End -> End | Elem(x,xs) -> xs;;

let rec naturals = function a -> function b -> if a >= b then End else Elem(a, naturals (a+1) b);;

let primes = function to_n ->
let rec sieve = function n ->
if n <= to_n
then n::sieve(n+1)
else []
and find_primes = function l ->
match l with h::t -> h:: find_primes(List.filter(fun x -> x mod h <> 0) t)
| [] -> [] in find_primes(sieve 2);;

let rec sil = function n -> match n with 0 -> 1 | n ->  n * sil (n - 1);;

let sin = function a ->
let rec fh = function i ->
if i > 10 then 0.0
else (a ** float_of_int i) /. float_of_int (sil i) -. ((a ** float_of_int (i+2)) /. float_of_int (sil (i+2))) +. fh (i+4)
in fh 1;;

let rec gen_list = function a -> function b -> function f -> if a >= b then [] else (f a)::(gen_list (a+1) b f);;
gen_list 0 91 (function i -> sin ((float_of_int i) *. 3.14 /. 90.));;
gen_list 0 11 (function i -> sin ((float_of_int (i*9)) *. 3.14 /. 90.));;

type 'a bt = Empty | Node of 'a * 'a bt * 'a bt;;
let t = Node(1,Node(2,Empty,Node(3,Empty,Empty)),Empty);;
let rec nodes = function n -> match n with Empty -> 0 | Node(_,t1,t2) -> 1 + nodes t1 + nodes t2;;

type 'a bt = Empty | Node of 'a * 'a bt * 'a bt;;
let rec append = function xs -> function ys -> match (xs,ys) with ([],_) -> ys | (x::tx,_) -> Elem(x, append tx ys);;
let (@) = append;;

let rec preorder = function n -> match n with Node(v,l,r) -> (v::((preorder l) @ (preorder r))) | Empty -> [];;
let rec preorder = function n -> match n with Node(v,l,r) -> Elem(v,(preorder l) @ (preorder r)) | Empty -> [];;

type a = A of (unit -> unit);;
A(function u -> if u = () then () else failwith "wtf");;


type 'a llist = LNil | LCons of 'a * (unit -> 'a llist);;
let lhd = function l -> match l with LNil -> failwith "lhd" | LCons(x,_) -> x;;
let ltl = function l -> match l with LNil -> failwith "ltl" | LCons (_,xf) -> xf();;
let rec lfrom = function k -> LCons(k, function u -> let () = u in lfrom (k+1));;
let rec ltake = function nl -> match nl with (0, _) -> [] | (_, LNil) -> [] | (n,LCons(x,xf)) -> x::(ltake(n - 1, xf()));;
ltake(5,lfrom 30);;

type 'a test = ONLA of 'a | OTHA of 'a best
and 'a best = ONLB of 'a | OTHB of 'a test;;
