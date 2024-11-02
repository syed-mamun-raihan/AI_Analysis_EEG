<html><head> 
<title>www.cs.tut.fi error message</title>
</head>
<body bgcolor="#d0d0d0">

<p> <font size="-1" color="gray"> 
This server is <code>www.cs.tut.fi</code>, located at 
<a href="http://www.cs.tut.fi/">the department of information technology</a>,
<a href="http://www.tut.fi/">Tampere university of technology</a>.
</font></p>
<hr>

<h1> ERROR </h1>

<p> Following error occured in execution of your request
"
<code>
GET http://www.cs.tut.fi/sgn/m2obsi/ex/kmeans.m (error code: 403)
</code>"
</p>
<hr>
<h3> Permission Denied </h3>
<p>
WWW server cannot read the document. File/path permissions or 
server access-lists deny access to this document.
</p>

<p> If you get this message from your own homepage within cs.tut.fi,
    please check: </p>
<ol>
    <li> Your html-file has READ access (<tt>chmod a+r FILE</tt>).
    <li> Your HOME-directory AND public_html BOTH have at least search
    access for the world (<tt>chmod o+x $HOME $HOME/public_html</tt>).
</ol>
<hr>
<address>
<a href="mailto:webmaster(at)cs.tut.fi">webmaster(at)cs.tut.fi</a>
</address>
</body>
</html>
