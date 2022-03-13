import subprocess
import json
import argparse
import os
import requests

args = None

def main():
    global args
    parser = argparse.ArgumentParser(
        description='update cloudflare dns records')

    parser.add_argument('Domain',
                        metavar='domain',
                        type=str,
                        help='domain to update')
    
    parser.add_argument('--dryrun',
                        action='store_true',
                        help='dry run')
    
    parser.add_argument('--kind',
                        action='store',
                        type=str,
                        default='A',
                        help='type of record, e.g. A')
    
    parser.add_argument('--zone',
                        action='store',
                        type=str,
                        help='dns zone')
    
    parser.add_argument('--id',
                        action='store',
                        type=str,
                        help='ID of record to update')

    parser.add_argument('--email',
                        action='store',
                        type=str,
                        help='email')

    parser.add_argument('--api-token',
                        action='store',
                        type=str,
                        help='API token')
    
    parser.add_argument('--api-key',
                        action='store',
                        type=str,
                        help='API key')
    
    parser.add_argument('--a-name',
                        action='store',
                        type=str,
                        help='record  A name')
    
    parser.add_argument('--a-ip',
                        action='store',
                        type=str,
                        help='record  A IP')
    
    args = parser.parse_args()

    domain = args.Domain

    if (args.api_token == None):
        atoken = os.getenv('CF_API_TOKEN')
        if (atoken == None):
            print("can't find CF_API_TOKEN")
            os.exit(1)
        args.api_token = atoken

    if (args.api_key == None):
        akey = os.getenv('CF_API_KEY')
        if (akey == None):
            print("can't find CF_API_KEY")
            os.exit(1)
        args.api_key = akey
        
    if (args.zone== None):
        azone = os.getenv('CF_ZONE_ID')
        if (azone== None):
            print("can't find CF_ZONE_ID")
            os.exit(1)
        args.zone = azone
        
    find_gcp_vms_ip(callback1)

def find_gcp_vms_ip(cb):
    instances = subprocess.Popen(['gcloud', 'compute', 'instances', 'list', '--format=json'], stdout=subprocess.PIPE).communicate()[0]
    instances2 = json.loads(instances.decode('utf-8').replace('\n', ' '))
    for inst in instances2:
        name = inst['name']
        interface = inst['networkInterfaces'][0]
        if (interface == None):
            return
        accessConfigs = interface['accessConfigs']
        if (accessConfigs == None):
            return
        externalIP = accessConfigs[0]['natIP']
        if (externalIP == None):
            return
        cb(name,externalIP)

def cloudflare_update(name,ip,domain):
    url = f"https://api.cloudflare.com/client/v4/zones/{args.zone}/dns_records"
#    print(url)
    bearer_token = f"Bearer {args.api_token}"

    headers = {
        'Authorization': bearer_token,
        'Content-Type': 'application/json',
        'Accept': 'application/json'
    }
#    print(bearer_token)
#        'X-Auth-Email': f"{args.email}",
#        'X-Auth-Key': f"{args.api_key}",

    response = requests.post(
        url,
        headers=headers,
        data = {
            'type': args.kind,
            'name': name,
            'content': ip,
            'priority': 10,
            'ttl':120,
            'proxied':'false'
        }
    )
    print(response.json())

def callback1(name,externalIP):
    #print(args)
    #print(name,externalIP)
    cloudflare_update(name,externalIP,args.Domain)

if __name__ == "__main__":
    main()
