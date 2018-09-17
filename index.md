# 인덱스

{% for page in site.pages %}
[{{page.title}}]({{ page.url | prepend: site.baseurl }})
{% endfor %}
