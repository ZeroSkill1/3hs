<!DOCTYPE html>
<html>
	<head>
		<meta charset="utf-8"/>
		<title>hLink | add to queue</title>
	</head>
	<body>
		[[if is-success?()]]
			<p>Added title [title-name] (with hShop ID [title-hshop-id]) to the queue.</p>
			<a href="/index.html">Back to home</a>
		[[else]]
			<p>An error occured: [error-message].</p>
		[[end]]
	</body>
</html>
