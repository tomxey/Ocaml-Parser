let rec sil = function x -> if x = 0 then 1 else x+sil(x-1);;
print_int ( sil 10 );;
