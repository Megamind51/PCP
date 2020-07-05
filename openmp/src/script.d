#pragma D option quiet

pcp*:::*enter
{
        @aux[probefunc] = vtimestamp;
}

pcp*:::*eturn
{
        @times[probefunc] += vtimestamp - @aux[probefunc];
}


END{
	printa(@times);
}