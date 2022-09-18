#include "workflow/WFFacilities.h"
#include <csignal>
#include "wfrest/HttpServer.h"

using namespace wfrest;

static WFFacilities::WaitGroup wait_group(1);

void sig_handler(int signo)
{
    wait_group.done();
}

int main()
{
    signal(SIGINT, sig_handler);

    HttpServer svr;

    // curl -v -X POST "ip:port/file_write1" -F "file=@filename" -H "Content-Type: multipart/form-data"
    svr.POST("/file_write1", [](const HttpReq *req, HttpResp *resp)
    {
        std::string& body = req->body();   // multipart/form - body has boundary
        resp->Save("test.txt", std::move(body));
    });

    svr.GET("/file_write2", [](const HttpReq *req, HttpResp *resp)
    {
        std::string content = "1234567890987654321";

        resp->Save("test1.txt", std::move(content));
    });

    // You can specify the message return to client when saving file successfully
    // default is "Save File success\n"
    svr.GET("/file_write3", [](const HttpReq *req, HttpResp *resp)
    {
        std::string content = "1234567890987654321";

        resp->Save("test2.txt", std::move(content), "test notify test successfully");
    });

    // You can specify the callback for save file task
    svr.GET("/file_write3", [](const HttpReq *req, HttpResp *resp)
    {
        std::string content = "1234567890987654321";

        resp->Save("test2.txt", std::move(content), [](WFFileIOTask *pwrite_task) {
            HttpServerTask *server_task = task_of(pwrite_task);
            HttpResp *resp = server_task->get_resp();

            if (pwrite_task->get_state() == WFT_STATE_SUCCESS)
            {
                resp->append_output_body_nocopy("Save File success\n", 18);
            }
        });
    });

    if (svr.start(8888) == 0)
    {
        wait_group.wait();
        svr.stop();
    } else
    {
        fprintf(stderr, "Cannot start server");
        exit(1);
    }
    return 0;
}
