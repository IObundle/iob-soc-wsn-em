
   wire pa_o;
   wire pa_i = dco_ckv;
   wire antena = pa_o;
   pa_tb pa_tb0
     (
      .in      (pa_i),
      .out     (pa_o),
      .pd      (pa_pd),
      .mode    (pa_mode)
      );

