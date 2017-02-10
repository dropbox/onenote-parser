package main

import (
       "errors"
       "strings"
       "encoding/json"
       "fmt"
       "io/ioutil"
       "os"
       "golang.org/x/net/html"
       "golang.org/x/net/html/atom"
)
type Element struct {
     Id string `json:id`
     Children []Element `json:children`
     Data string `json:"#data"`
     Scalar interface{} `json:"#scalar"`
     Base64 string `json:"#base64"`
     Style map[string]interface{} `json:style`
}
func (eself *Element)parseStyle() []html.Attribute{
     var retval []html.Attribute
     if len(eself.Style) == 0 {
     	return retval
     }
     retval = append(retval, html.Attribute{Key:"style"})
     for key, val := range eself.Style {
     	 if key[0] == '#' {
	    continue
	 }
     	 var valStr string
	 if vals, ok := val.(string); ok {
	    valStr = vals
	 } else {
	    valStr = fmt.Sprintf("%v", val)
	 }
     	 retval[0].Val += key + "=" + strings.Replace(valStr, "\x00", "", -1) + ";"
     }
     return retval
}
func (eself *Element)hasText() bool{
     if eself.Id == "#text" {
     	return true
     }
     for _, child := range eself.Children {
     	 if child.hasText() {
	    return true
	 }
     }
     return false
}
func (eself *Element)toHtml() *html.Node{
     if len(eself.Id) != 0 {
     	if eself.Id == "#text" {
	   return &html.Node {
	   	  Data:eself.Data,
		  Type:html.TextNode,
	   }
	} else {
	  var retval html.Node
	  if eself.Id == "li" && !eself.hasText() { // HAX
	     return nil
	  }
	  retval.Data = eself.Id
	  retval.Type = html.ElementNode
	  retval.DataAtom = atom.Lookup([]byte(eself.Id)) // FIXME tolower?
	  retval.Attr = append(retval.Attr,
	  	eself.parseStyle()...)
	  for _, child := range eself.Children {
	      htmlChild := child.toHtml()
	      if htmlChild != nil {
	      	 retval.AppendChild(htmlChild)
	      }
	  }
	  return &retval
	}
     }
     return nil
}
const emptyDocument string = `<!DOCTYPE html>
<html lang="en"><head>
    <meta charset="utf-8">
    <!--<link rel="stylesheet" href="style.css">-->
  </head><body></body></html>
  `

func findHeadBody(root *html.Node) (head,body *html.Node, err error){
     cur := root
     stack := []*html.Node{}
     for cur != nil && (body == nil || head == nil) {
          if cur.DataAtom == atom.Body {
     	  	body = cur
	  }
     	  if cur.DataAtom == atom.Head {
	    	head = cur
     	  }
	  if cur.NextSibling != nil {
	  	  stack = append(stack, cur.NextSibling)
	  }
 	  if cur.FirstChild != nil {
	     cur = cur.FirstChild
	  } else if len(stack) != 0{
	    cur = stack[len(stack) - 1]
	    stack = stack[:len(stack) - 1]
	  } else {
	    cur = nil
	  }
     }
     
     if body == nil {
     	err = errors.New("Could not locate body")
     }
     if head == nil {
     	err = errors.New("Could not locate head")
     }
     return
}
func main() {
     input, err := ioutil.ReadAll(os.Stdin)
     if err != nil {
     	panic(err)
     }
     var dat []Element
     err = json.Unmarshal(input, &dat)
     if err != nil {
     	panic(err)
     }
     root, parseErr := html.Parse(strings.NewReader(emptyDocument))
     if parseErr != nil {
     	panic(parseErr)
     }
     head, body, findErr := findHeadBody(root)
     if findErr != nil {
     	panic(findErr)
     }
     /*
     head.AppendChild(&html.Node {
     			   Type:html.ElementNode,
			   DataAtom:atom.Link,
			   Data:"link",
			   Attr:[]html.Attribute {
			   	html.Attribute{
					Key:"rel",
					Val:"stylesheet",
				},
			   	html.Attribute{
					Key:"href",
					Val:"style.css",
				},
			   },
			   })
     
     body.AppendChild(&html.Node {
     			   Type:html.TextNode,
			   Data:"HELLOTE",
			   })
     */
     _ = head
     for _, ele := range dat {
     	  htmlElement := ele.toHtml()
	  if htmlElement != nil {
             body.AppendChild(htmlElement)
	  }
     }
     err = html.Render(os.Stdout, root)
     if err != nil {
          panic(err)
     }
     _,_ = os.Stdout.Write([]byte{'\n'})
}