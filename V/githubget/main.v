import net.http
import os
import json

struct OrgResp {
	login       string
	id          int
	members_url string
	hooks_url   string
	events_url  string
}

struct MembersResp {
	login     string
	repos_url string
	gists_url string
}

struct OwnerResp {
	followers int
}

struct RepoResp {
	id               int
	name             string
	full_name        string
	private          bool
	html_url         string
	description      string
	stargazers_count int
	forks_count      int
	owner            OwnerResp
}

fn main() {
	if os.args.len < 2 {
		eprintln('organization name required')
		exit(1)
	}
	orgname := os.args[1]
	gh_token := os.getenv('GITHUB_TOKEN')
	if gh_token.len == 0 {
		eprintln('no github token env var')
		exit(1)
	}
	// println('$gh_token')
	apiurl := 'https://api.github.com/'
	mut url := apiurl + 'orgs/' + orgname
	mut resp := http.get(url)?
	// println(resp.text)
	/*
	org := json.decode(OrgResp, resp.text) or {
		eprintln('Failed to parse json')
		exit(1)
	}
	*/
	// println(org)
	mut totalmembers := []MembersResp{}
	limit := 100
	for i := 1; i < limit; i++ { // XXX
		url = apiurl + 'orgs/' + orgname + '/members?page=$i&per_page=$limit'
		resp = http.fetch(url, {
			method: .get
			headers: {
				'Authorization': 'token $gh_token'
			}
		})?
		// println(resp)
		members := json.decode([]MembersResp, resp.text) or {
			eprintln('Failed to parse members')
			exit(1)
		}
		if members.len == 0 {
			break
		}
		// println('adding $members.len')
		totalmembers << members
	}
	// println(totalmembers)
	println('number of members $totalmembers.len')
	mut memb_with_repos := 0
	mut max_stars := 0
	for memb in totalmembers {
		resp = http.fetch(memb.repos_url, {
			method: .get
			headers: {
				'Authorization': 'token $gh_token'
			}
		})?
		repos := json.decode([]RepoResp, resp.text) or {
			eprintln('Failed to parse repos')
			exit(1)
		}
		if repos.len > 0 {
			for repo in repos {
				if repo.stargazers_count > max_stars {
					max_stars = repo.stargazers_count
					println('$memb.login followeby by $repo.owner.followers $repo.html_url stars $repo.stargazers_count')
				}
			}
			// println('$memb.login $repos.len')
			memb_with_repos++
			if repos.len >= 30 {
				println('$memb.login has $repos.len repos')
			}
		}
	}
	println('members with repos $memb_with_repos max stars $max_stars')
}
