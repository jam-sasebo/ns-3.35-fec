#!/usr/bin/perl

#tcp
@delay = (100);
@nFlows = (10);
@fwin = (20);
#@fwin = (1,3,5,7,9,10,15,20,25,30);
@nGroups = (1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20);
#@nGroups = (1,3,5,7,9,10,15,20,25,30);

 for($f=0;$f<=$#nFlows;$f++){  
 for($d=0;$d<=$#delay;$d++){  
 for($w=0;$w<=$#fwin;$w++){  
 for($g=0;$g<=$#nGroups;$g++){  

 $file="fec-end-f${nFlows[$f]}-d${delay[$d]}-w${fwin[$w]}-g${nGroups[$g]}-thp.dat";    
 open(IN_FILE, $file) || die $!;
 $y=0;
 
 $THP = 0;			
 $SUM_THP = 0;
 $AVE_THP = 0;
 $cnt = 0;

 while(<IN_FILE>){
 ($thp_name[$y], $thp[$y], $c[$y], $d[$y], $e[$y]) = split;
   if ($thp[$y] > 1)
     {
       $THP+=$thp[$y];
       $cnt++;
     }
   $y++;
 }
 
 $SUM_THP = $THP;
 $AVE_THP = $SUM_THP / ${nFlows[$f]}; 
 print "f $nFlows[$f] d $delay[$d] w $fwin[$w] g $nGroups[$g] sum $SUM_THP ave $AVE_THP\n";
}
}
}
}

