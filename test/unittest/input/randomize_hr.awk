#!/bin/gawk -f

function randint(start, end)
{
    return start + int((end - start) * rand())
}

function randfloat(start, end)
{
    return start + ((end - start) * rand())
}

// {
	matched=0;
}

match($0, /^(\s*)<gpxtpx:hr>([0-9]+(\.[0-9]+)?)<\/gpxtpx:hr>\s*$/, ary) {
	print ary[1] "<gpxtpx:hr>" randint(100, 200) "</gpxtpx:hr>";
	matched=1;
}

// {
	if(0 == matched)
	{
		print $0;
	}
}
