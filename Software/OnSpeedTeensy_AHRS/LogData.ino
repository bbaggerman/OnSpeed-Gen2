void logData()
{
unsigned long timeStamp=millis(); // save timestamp for logging  
char logLine[2048];
int charsAdded=0;
int boomAge=0;
int efisAge=0;
if (sdLogging)
        {
          charsAdded+=sprintf(logLine, "%lu,%i,%.2f,%i,%.2f,%.2f,%.2f,%.2f,%.2f,%i,%i",timeStamp,Pfwd,PfwdSmoothed,P45,P45Smoothed,Pstatic,Palt,IAS,AOA,flapsPos,dataMark);
          //charsAdded+=sprintf(logLine, "%lu,%i,%.2f,%i,%.2f,%.2f,%.2f,%.2f,%.2f,%i,%i",timeStamp,124,124.56,145,145.00,1013.00,5600.00,110.58,10.25,2,0);
          yield(); // process serial interrupts
          charsAdded+= sprintf(logLine+charsAdded, ",%.2f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.1f,%.1f",imuTemp,Az,Ay,Ax,Gx,Gy,Gz,smoothedPitch,smoothedRoll);
          yield();
          if (readBoom)
            { 
            boomAge=millis()-boomTimestamp;            
            charsAdded+=sprintf(logLine+charsAdded, ",%.2f,%.2f,%.2f,%.2f,%.2f,%i",boomStatic,boomDynamic,boomAlpha,boomBeta,boomIAS,boomAge);
            yield();
            }         
          if (readEfisData)                         
            {
            efisAge=millis()-efisTimestamp;  
              if (efisType.substring(0,3)=="VN-") // VN-300 type data
                  {
                  #ifdef VNDEBUG
                  Serial.printf("\nvnAngularRateRoll: %.2f,vnAngularRatePitch: %.2f,vnAngularRateYaw: %.2f,vnVelNedNorth: %.2f,vnVelNedEast: %.2f,vnVelNedDown: %.2f,vnAccelFwd: %.2f,vnAccelLat: %.2f,vnAccelVert: %.2f,vnYaw: %.2f,vnPitch: %.2f,vnRoll: %.2f,vnLinAccFwd: %.2f,vnLinAccLat: %.2f,vnLinAccVert: %.2f,vnYawSigma: %.2f,vnRollSigma: %.2f,vnPitchSigma: %.2f,vnGnssVelNedNorth: %.2f,vnGnssVelNedEast: %.2f,vnGnssVelNedDown: %.2f,vnGPSFix: %i,TimeUTC: %s,vnGnssLat: %.6f,vnGnssLon: %.6f\n",vnAngularRateRoll,vnAngularRatePitch,vnAngularRateYaw,vnVelNedNorth,vnVelNedEast,vnVelNedDown,vnAccelFwd,vnAccelLat,vnAccelVert,vnYaw,vnPitch,vnRoll,vnLinAccFwd,vnLinAccLat,vnLinAccVert,vnYawSigma,vnRollSigma,vnPitchSigma,vnGnssVelNedNorth,vnGnssVelNedEast,vnGnssVelNedDown,vnGPSFix,vnTimeUTC.c_str(),vnGnssLat,vnGnssLon);                  
                  #endif
                  charsAdded+=sprintf(logLine+charsAdded,",%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.6f,%.6f,%i,%i,%s",vnAngularRateRoll,vnAngularRatePitch,vnAngularRateYaw,vnVelNedNorth,vnVelNedEast,vnVelNedDown,vnAccelFwd,vnAccelLat,vnAccelVert,vnYaw,vnPitch,vnRoll,vnLinAccFwd,vnLinAccLat,vnLinAccVert,vnYawSigma,vnRollSigma,vnPitchSigma,vnGnssVelNedNorth,vnGnssVelNedEast,vnGnssVelNedDown,vnGnssLat,vnGnssLon,vnGPSFix,efisAge,vnTimeUTC.c_str());                  
                  yield();
                  } else                  
                        {
                        charsAdded+=sprintf(logLine+charsAdded, ",%.2f,%.2f,%.2f,%.2f,%.2f,%i,%i,%i,%.2f,%.2f,%.2f,%.2f,%.2f,%i,%i,%i,%i,%s",efisIAS,efisPitch,efisRoll,efisLateralG,efisVerticalG,efisPercentLift,efisPalt,efisVSI,efisTAS,efisOAT,efisFuelRemaining,efisFuelFlow,efisMAP,efisRPM,efisPercentPower,efisHeading,efisAge,efisTime.c_str());
                        yield();
                        }                        
            }               
          charsAdded+=sprintf(logLine+charsAdded, ",%.2f,%.2f,%.2f,%.2f",earthVertG,flightPath,kalmanAlt*3.28084,kalmanVSI*196.85);
          yield();
          sprintf(logLine+charsAdded,"\n");
  //      Serial.print("8");        

#ifdef AGEDEBUG
      Serial.printf("%i,%i\n",boomAge,efisAge);
#endif          
          
        datalogRingBufferAdd(logLine);
        yield();      
        }              
}



void createLogFile()
{
 //sdAvailable=Sd.begin(SdioConfig(FIFO_SDIO));
 if (dataSource=="SENSORS" && sdAvailable && sdLogging)
        {
        Serial.println("SD card initialized. Logging Enabled.");       
        sprintf(filenameSensor,"log_1.csv");        
        int fileCount=1;
        while (Sd.exists(filenameSensor))
              {
              fileCount++;  
              sprintf(filenameSensor,"log_%d.csv",fileCount);              
              }
        Serial.print("Sensor log file:"); Serial.println(filenameSensor);        
        if (sdLogging)
            {
            SensorFile = Sd.open(filenameSensor, O_CREAT | O_WRITE | O_TRUNC);
            if (SensorFile) {
                            SensorFile.print("timeStamp,Pfwd,PfwdSmoothed,P45,P45Smoothed,PStatic,Palt,IAS,AngleofAttack,flapsPos,DataMark");
                            SensorFile.print(",imuTemp,VerticalG,LateralG,ForwardG,RollRate,PitchRate,YawRate,Pitch,Roll");
                            if (readBoom) SensorFile.print(",boomStatic,boomDynamic,boomAlpha,boomBeta,boomIAS,boomAge");       
                            if (readEfisData)
                                    {
                                    if (efisType.substring(0,3)=="VN-") // VN-300 type data
                                                SensorFile.print(",vnAngularRateRoll,vnAngularRatePitch,vnAngularRateYaw,vnVelNedNorth,vnVelNedEast,vnVelNedDown,vnAccelFwd,vnAccelLat,vnAccelVert,vnYaw,vnPitch,vnRoll,vnLinAccFwd,vnLinAccLat,vnLinAccVert,vnYawSigma,vnRollSigma,vnPitchSigma,vnGnssVelNedNorth,vnGnssVelNedEast,vnGnssVelNedDown,vnGnssLat,vnGnssLon,vnGPSFix,vnDataAge,vnTimeUTC");
                                                else
                                                    SensorFile.print(",efisIAS,efisPitch,efisRoll,efisLateralG,efisVerticalG,efisPercentLift,efisPalt,efisVSI,efisTAS,efisOAT,efisFuelRemaining,efisFuelFlow,efisMAP,efisRPM,efisPercentPower,efisMagHeading,efisAge,efisTime");
                                    }
                            SensorFile.print(",EarthVerticalG, FlightPath, KalmanAlt, KalmanVSI");
                            SensorFile.println();
                            SensorFile.close();
                            }  else
                                    {
                                    Serial.println("SensorFile opening error. Logging disabled.");        
                                    sdLogging=false;
                                    }
            }
         
        
      } 
}