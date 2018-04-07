#!/usr/bin/env perl

$a=`pidof \"Cliente TCP\"`;
$b=`pidof \"Cliente UDP\"`;
$c=`pidof \"Servidor_TCP\"`;
$d=`pidof \"Servidor_UDP\"`;

sub finalizar{
  foreach my $x (@_) {
    system("kill -9 $x");
  }
}

finalizar($a);
finalizar($b);
finalizar($c);
finalizar($d);
