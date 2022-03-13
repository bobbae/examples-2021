# cloudflare dns record update example

Unfortunately it does not work with some domains

```
      "message": "You cannot use this API for domains with a .cf, .ga, .gq, .ml, or .tk TLD (top-level domain). To configure the DNS settings for this domain, use the Cloudflare Dashboard."
```

## netlify

Use netlify name servers to update via REST API. It does not have 
restrictions for freenom domains like .cf, .ga, .gq, .ml or .tk.

```
# get zones

curl  -H "Authorization: Bearer ${NETLIFY_TOKEN}" https://api.netlify.com/api/v1/dns_zones|jq .

# using one of the zones, update A record

curl -XPOST -H "Authorization: Bearer ${NETLIFY_TOKEN}" -H "Content-Type: application/json" https://api.netlify.com/api/v1/dns_zones/${NETLIFY_ZONE_ID}/dns_records -d '{ "type":"A", "hostname":"mynode", "value":"34.34.23.123" }'
```
