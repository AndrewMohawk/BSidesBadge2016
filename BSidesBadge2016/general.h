
boolean addChallenge(String c_name)
{
  boolean found = false;
  for(int i=0;i<completedChallenges;i++)
  {
    //Serial.print("Does ");Serial.print(Challenges[i]);Serial.print("=");Serial.print(c_name);Serial.print("?");
    if(Challenges[i] == c_name)
    {
      found = true;
      //Serial.println("yes");
      //Serial.println("Found challenge!");
    }
  
  }

  if(found == false)
  {
    
    Challenges[completedChallenges] = c_name;
    completedChallenges++;
    currentListedChallenge = 0;
    return true;
  }
  else
  {
    
    return false;
  }
}
