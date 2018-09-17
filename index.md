# 인덱스

{% for page in site.static_pages %}
[{{page.title}}]({{ page.url | prepend: site.baseurl }})
{% endfor %}
