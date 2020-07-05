#pragma D option quiet

pcp*:::*-enter
{
        @aux[probefunc] = vtimestamp;
}

pcp*:::*-return
{
        @times[probefunc] += vtimestamp - @aux[probefunc];
}


END{
	printa(@times);
}