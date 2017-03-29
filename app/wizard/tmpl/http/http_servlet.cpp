#include "stdafx.h"
#include "http_servlet.h"

http_servlet::http_servlet(acl::socket_stream* stream, acl::session* session)
: acl::HttpServlet(stream, session)
{

}

http_servlet::~http_servlet(void)
{

}

bool http_servlet::doError(acl::HttpServletRequest&,
	acl::HttpServletResponse& res)
{
	res.setStatus(400);
	res.setContentType("text/xml; charset=utf-8");

	// ���� http ��Ӧ��
	acl::string buf;
	buf.format("<root error='some error happened!' />\r\n");
	res.write(buf);
	res.write(NULL, 0);
	return false;
}

bool http_servlet::doOther(acl::HttpServletRequest&,
	acl::HttpServletResponse& res, const char* method)
{
	res.setStatus(400);
	res.setContentType("text/xml; charset=utf-8");
	// ���� http ��Ӧ��
	acl::string buf;
	buf.format("<root error='unkown request method %s' />\r\n", method);
	res.write(buf);
	res.write(NULL, 0);
	return false;
}

bool http_servlet::doGet(acl::HttpServletRequest& req,
	acl::HttpServletResponse& res)
{
	return doPost(req, res);
}

bool http_servlet::doPost(acl::HttpServletRequest& req,
	acl::HttpServletResponse& res)
{
	// �����Ҫ http session ���ƣ��������ע�ͣ�����Ҫ��֤
	// �� master_service.cpp �ĺ��� thread_on_read �����õ�
	// memcached ������������
	/*
	const char* sid = req.getSession().getAttribute("sid");
	if (*sid == 0)
		req.getSession().setAttribute("sid", "xxxxxx");
	sid = req.getSession().getAttribute("sid");
	*/

	// �����Ҫȡ������� cookie �������ע��
	/*
	$<GET_COOKIES>
	*/

	res.setContentType("text/xml; charset=utf-8")	// ������Ӧ�ַ���
		.setKeepAlive(req.isKeepAlive())	// �����Ƿ񱣳ֳ�����
		.setContentEncoding(true)		// �Զ�֧��ѹ������
		.setChunkedTransferEncoding(true);	// ���� chunk ���䷽ʽ

	const char* param1 = req.getParameter("name1");
	const char* param2 = req.getParameter("name2");

	// ���� xml ��ʽ��������
	acl::xml1 body;
	body.get_root()
		.add_child("root", true)
			.add_child("params", true)
				.add_child("param", true)
					.add_attr("name1", param1 ? param1 : "null")
				.get_parent()
				.add_child("param", true)
					.add_attr("name2", param2 ? param2 : "null");
	acl::string buf;
	body.build_xml(buf);

	// ���� http ��Ӧ�壬��Ϊ������ chunk ����ģʽ��������Ҫ�����һ��
	// res.write ������������Ϊ 0 �Ա�ʾ chunk �������ݽ���
	return res.write(buf) && res.write(NULL, 0);
}
