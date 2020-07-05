provider pcp
{
  probe aloc_pgm_enter();
  probe aloc_pgm_return();
  probe read_pgm_enter();
  probe read_pgm_return();
  probe write_pgm_enter();
  probe write_pgm_return();
  probe run_parallel_enter();
  probe run_parallel_return();
  probe main_enter();
  probe main_return();
};