$(document).ready(function() {
    $("a").each(function() {
        var text = $(this).text();
        $(this).text(text.replace("operator &", "operator&"));
    });
    $("td.memTemplParams, .memtemplate").each(function() {
        var text = $(this).text();
        text = text.replace(/template\</g, "template <")
        text = text.replace(/([a-zA-Z0-9_:]+)[ ]+([,>])/g, "$1$2")
        $(this).text(text);
    });
    $("td.memItemLeft, td.memTemplItemLeft, td.memItemRight, td.memTemplItemRight").each(function() {
        var nodes = [];
        for (var index = 0; index < this.childNodes.length; index++) {
            var child = this.childNodes[index];
            if (child.nodeType == 3) {
                nodes.push(child);
            }
        }
        for (var index = 0; index < nodes.length; index++) {
            nodes[index].parentNode.replaceChild(
                document.createTextNode(nodes[index].nodeValue
                    .replace(/[ ]+([&*])([a-zA-Z_])/g, "$1 $2")
                    .replace(/-\>/, "→")
                    .replace(/\<[ ]+/g, "<")
                    .replace(/[ ]+\>/g, ">")), nodes[index]);
        }
    });
    $(".classindex").wrap("<div style='overflow-x:auto'></div>");
});
