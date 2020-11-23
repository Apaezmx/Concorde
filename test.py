import requests
import time
from requests.adapters import HTTPAdapter
from requests.packages.urllib3.util.retry import Retry


s = requests.Session()
retries = Retry(total=500, backoff_factor=1, status_forcelist=[500, 502, 503, 504 ])
times = []
for i in range(10000):
     start = time.time()
     r = s.get("http://localhost:2235/index")
     times.append(time.time() - start)
     print(str(r.status_code) + " "  + str(r.content))

print(sum(times))
print(sum(times)/len(times))
