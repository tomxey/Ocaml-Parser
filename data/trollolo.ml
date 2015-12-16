let rec sil = function x -> if x = 0 then 1 else x+sil(x-1);;
print_int ( sil 10 );;


let test = 1.0;;
let testf = function n -> 1;;
print_int (testf 1);;
let id = function x -> x;;
id 5;;
test;;
