package slackgist

import (
	"fmt"

	"github.com/parnurzeal/gorequest"
)

type field struct {
	Title string `json:"title"`
	Value string `json:"value"`
	Short bool   `json:"short"`
}

type slackAttachment struct {
	Fallback   *string   `json:"fallback"`
	Color      *string   `json:"color"`
	PreText    *string   `json:"pretext"`
	AuthorName *string   `json:"author_name"`
	AuthorLink *string   `json:"author_link"`
	AuthorIcon *string   `json:"author_icon"`
	Title      *string   `json:"title"`
	TitleLink  *string   `json:"title_link"`
	Text       *string   `json:"text"`
	ImageUrl   *string   `json:"image_url"`
	Fields     []*field  `json:"fields"`
	Footer     *string   `json:"footer"`
	FooterIcon *string   `json:"footer_icon"`
	Timestamp  *int64    `json:"ts"`
	MarkdownIn *[]string `json:"mrkdwn_in"`
}

type slackPayload struct {
	Parse       string            `json:"parse,omitempty"`
	Username    string            `json:"username,omitempty"`
	IconUrl     string            `json:"icon_url,omitempty"`
	IconEmoji   string            `json:"icon_emoji,omitempty"`
	Channel     string            `json:"channel,omitempty"`
	Text        string            `json:"text,omitempty"`
	LinkNames   string            `json:"link_names,omitempty"`
	Attachments []slackAttachment `json:"attachments,omitempty"`
	UnfurlLinks bool              `json:"unfurl_links,omitempty"`
	UnfurlMedia bool              `json:"unfurl_media,omitempty"`
	Markdown    bool              `json:"mrkdwn,omitempty"`
}

func (attachment *slackAttachment) AddField(f field) *slackAttachment {
	attachment.Fields = append(attachment.Fields, &f)
	return attachment
}

func redirectPolicyFunc(req gorequest.Request, via []gorequest.Request) error {
	return fmt.Errorf("Incorrect token (redirection)")
}

func sendSlack(webhookUrl string, proxy string, payload slackPayload) []error {
	request := gorequest.New().Proxy(proxy)
	resp, _, err := request.
		Post(webhookUrl).
		RedirectPolicy(redirectPolicyFunc).
		Send(payload).
		End()
	if err != nil {
		return err
	}
	if resp.StatusCode >= 400 {
		return []error{fmt.Errorf("Error sending msg. Status: %v", resp.Status)}
	}
	return nil
}
