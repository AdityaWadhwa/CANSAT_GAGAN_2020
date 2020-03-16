void update_state()
{
  /*
   0 for boot
   1 Idle
   2 Ascent
   3 Cansat Deployment
   4 Descent
   5 Payload Deployment
   6 Parachute Deployment 
   7 End
  */

   if(state==0 && pres_alt==prev_alt && pres_alt<5) //still inside rocket
      state=1;                                      //idle

   else if(state==1 && pres_alt>prev_alt)           //inside rocket
      state=2;                                      //Launch Detect

   else if(state==2 && analogRead(A1)<100)          //LDR shows cansat is outside
      state=3;                                      //Cansat Deployement 

   else if(state==3 && pres_alt<prev_alt)           //payload inside container descending
      state=4;                                      //Descent

   else if(state==4 && pres_alt<=450)               //release the payload
      state=5;                                      //Payload Deployement

   else if(state==5 && pres_alt<=100)               //release payload parachute
      state=6;

   else if(state==6 && pres_alt<=5)
      state=7;
}
