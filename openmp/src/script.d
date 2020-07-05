#pragma D option quiet

pcp*:::*entry
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