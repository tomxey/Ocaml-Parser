fst ((tuple 1) "string");;
snd ((tuple 1) "string");;

let swap = function p -> (tuple (snd p)) (fst p);;
swap ((tuple 1) "string");;
