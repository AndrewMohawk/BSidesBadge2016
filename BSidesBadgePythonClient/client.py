import urllib2,urllib,json,requests,sys
from random import randint
class BSidesBadge(object):
    #code
    badge_name = ""
    baseURL = "http://localhost:8000/badge/"
    badge_hash = ""
    badges_seen = []
    
    def __init__(self,badgeName):
        self.badge_name = badgeName
        self.badges_seen = []
    
    def addBadge(self,badgeName):
        if badgeName not in self.badges_seen and badgeName != self.badge_name:
            self.badges_seen.append(badgeName)
        #else:
            #print "could not add: %s" % ( badgeName )
    
    def getHash(self):
        url = self.baseURL + "gethash/" + self.badge_name + "/"
        print "browsing to %s" % url
        response = urllib2.urlopen(url)
        data = response.read()
        self.badge_hash = data
        print "Hash: %s" %(data)
    
    def checkin(self):
        url = self.baseURL + "checkin/" + self.badge_name + "/"
        print "browsing to %s" % url
        '''
        
        data = urllib.urlencode({'seen' :  json.dumps(self.badges_seen)})
        response = urllib2.urlopen(url,data=data)
        data = response.read()
        print "Page response for badge: %s\n%s" %(self.badge_name,data)
        '''
        postData = {'seen' :  json.dumps(self.badges_seen)}
        #print self.badges_seen
        r = requests.post(url, data=postData, allow_redirects=True)
        data = r.content
        return data
        
    def decryptPage(self,page):
        decryptedMessage = ""
        shiftLen = len(self.badge_hash)
		#print "!!!%s!!" % (message)
        for x in page:
            decryptedMessage = decryptedMessage +  chr(ord(x) + shiftLen)
        return decryptedMessage
    
    def process(self):
        # fetch the current hash
        self.getHash()
        #fetch the page
        cryptedPage = self.checkin()
        #decrypt
        decryptedData = self.decryptPage(cryptedPage)
        #parse data
        print decryptedData + "\n\n"
        
badgeName = sys.argv[1]
numBadges = sys.argv[2]

testBadges = []
testBadges = [ BSidesBadge(badgeName + str(i)) for i in range(int(numBadges))]
if(len(sys.argv) > 3):
        
        for x in range(0,len(testBadges)):
            maximum_seen_badges = len(testBadges)
            z = randint(0, maximum_seen_badges)
            z = 5; # maximum for how many badges one has seen at one time!
            #print "Adding %s badges to badge %s" % (z,x)
            for c in range(0,z):
                seenBadge = testBadges[randint(0,len(testBadges)-1)].badge_name
                testBadges[x].addBadge(seenBadge)
for x in range(0,len(testBadges)):
    testBadges[x].process()

