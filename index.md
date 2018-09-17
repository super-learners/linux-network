{% assign sorted_pages = site.static_pages | sort: "title" %}
{% for page in sorted_pages %}
[{{page.title}}]({{ page.url | prepend: site.baseurl }})
{% endfor %}
