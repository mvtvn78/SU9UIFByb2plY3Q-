import React, { useEffect, useState } from 'react';
import axios from 'axios';
import {
  Box,
  Button,
  Card,
  CardContent,
  Container,
  Divider,
  TextField,
  Typography,
  Dialog,
  DialogTitle,
  DialogContent,
  DialogActions,
  List,
  ListItem,
  ListItemText,
} from "@mui/material";
import SendIcon from '@mui/icons-material/Send';
import AddIcon from '@mui/icons-material/Add';
import VisibilityIcon from '@mui/icons-material/Visibility';
function App() {
  const [devices, setDevices] = useState([]);
  const [newDevice, setNewDevice] = useState({ name: "", topic: "" });
  const [payloads, setPayloads] = useState({});
  const [telemetry, setTelemetry] = useState([]);
  const [selectedDevice, setSelectedDevice] = useState(null);
  const [openDialog, setOpenDialog] = useState(false);

  // thêm state để debug / hiển thị trạng thái
  const [telemetryLoading, setTelemetryLoading] = useState(false);
  const [telemetryError, setTelemetryError] = useState(null);

  useEffect(() => {
    fetchDevices();
  }, []);

  const fetchDevices = async () => {
    try {
      const res = await axios.get("http://localhost:8080/devices");
      setDevices(res.data || []);
      const initPayloads = {};
      (res.data || []).forEach((d) => {
        initPayloads[d.id] = "";
      });
      setPayloads(initPayloads);
    } catch (error) {
      console.error("Lỗi khi tải danh sách thiết bị:", error);
    }
  };

  const handleSend = async (id) => {
    const payload = payloads[id];
    if (!payload) return;
    try {
      await axios.post(`http://localhost:8080/devices/${id}/control`, payload, {
        headers: { "Content-Type": "text/plain" },
      });
      // alert("Lệnh đã gửi thành công!");
    } catch (error) {
      console.error("Lỗi gửi lệnh:", error);
      alert("Gửi thất bại!");
    }
  };

  const handleCreate = async () => {
    if (!newDevice.name || !newDevice.topic) return;
    try {
      await axios.post("http://localhost:8080/devices", newDevice);
      setNewDevice({ name: "", topic: "" });
      fetchDevices();
    } catch (error) {
      console.error("Lỗi tạo thiết bị:", error);
    }
  };

  const fetchTelemetry = async (deviceId) => {
    try {
      const res = await axios.get(
        `http://localhost:8080/telemetry/${deviceId}`
      );
      console.log("Telemetry từ API:", res.data);
      // Đảm bảo luôn trả về mảng
      if (Array.isArray(res.data)) {
        return res.data;
      }
      if (res.data == null) {
        return [];
      }
      // Nếu backend trả về object đơn thì bọc lại thành mảng
      return [res.data];
    } catch (error) {
      console.error("Lỗi tải telemetry:", error);
      throw error;
    }
  };

  const handleViewTelemetry = async (device) => {
    setSelectedDevice(device);
    setOpenDialog(true);
    setTelemetry([]);
    setTelemetryError(null);
    setTelemetryLoading(true);

    try {
        const data = await fetchTelemetry(device.id); // (device.id)
      setTelemetry(data);
    } catch (error) {
      setTelemetryError("Không tải được dữ liệu telemetry từ server.");
    } finally {
      setTelemetryLoading(false);
    }
  };

  return (
    <Container maxWidth="md" sx={{ py: 4 }}>
      {/* Header */}
      <Typography
        variant="h4"
        align="center"
        gutterBottom
        sx={{ fontWeight: "bold", color: "#1976d2" }}
      >
        📡 IoT Device Dashboard
      </Typography>

      {/* Phần 1: Danh sách thiết bị */}
      <Box sx={{ mb: 4 }}>
        <Typography
          variant="h6"
          sx={{ mb: 2, display: "flex", alignItems: "center" }}
        >
          📋 Danh sách thiết bị
        </Typography>

        {devices.length === 0 ? (
          <Typography color="text.secondary" align="center">
            Chưa có thiết bị nào.
          </Typography>
        ) : (
          devices.map((device) => (
            <Card key={device.id} sx={{ mb: 2, boxShadow: 3 }}>
              <CardContent>
                <Typography variant="h6" component="div">
                  {device.name}
                </Typography>
                <Typography sx={{ mb: 1.5 }} color="text.secondary">
                  MQTT Topic: <code>{device.topic}</code>
                </Typography>

                <TextField
                  label="Nhập lệnh (Payload)"
                  variant="outlined"
                  size="small"
                  fullWidth
                  value={payloads[device.id] || ""}
                  onChange={(e) =>
                    setPayloads({ ...payloads, [device.id]: e.target.value })
                  }
                  sx={{ mt: 1, mb: 2 }}
                />

                <Box sx={{ display: "flex", gap: 2 }}>
                  <Button
                    variant="contained"
                    color="primary"
                    onClick={() => handleSend(device.id)}
                    endIcon={<SendIcon />}
                    sx={{ textTransform: "none", flex: 1 }}
                  >
                    GỬI LỆNH
                  </Button>

                  <Button
                    variant="outlined"
                    color="info"
                    onClick={() => handleViewTelemetry(device)}
                    startIcon={<VisibilityIcon />}
                    sx={{ textTransform: "none", flex: 1 }}
                  >
                    XEM DỮ LIỆU
                  </Button>
                </Box>
              </CardContent>
            </Card>
          ))
        )}
      </Box>

      <Divider sx={{ my: 4 }} />

      {/* Phần 2: Thêm thiết bị mới */}
      <Box>
        <Typography variant="h6" sx={{ mb: 2 }}>
          ➕ Thêm thiết bị mới
        </Typography>
        <Card variant="outlined" sx={{ p: 2 }}>
          <Box sx={{ display: "flex", flexDirection: "column", gap: 2 }}>
            <TextField label="Tên thiết bị (VD: Đèn Bếp)"
              variant="outlined"
              size="small"
              value={newDevice.name}
              onChange={(e) =>
                setNewDevice({ ...newDevice, name: e.target.value })
              }
            />
            <TextField
              label="MQTT Topic (VD: lab306/den2)"
              variant="outlined"
              size="small"
              value={newDevice.topic}
              onChange={(e) =>
                setNewDevice({ ...newDevice, topic: e.target.value })
              }
            />
            <Button
              variant="contained"
              color="success"
              startIcon={<AddIcon />}
              onClick={handleCreate}
              sx={{ textTransform: "none", alignSelf: "flex-start" }}
            >
              TẠO THIẾT BỊ
            </Button>
          </Box>
        </Card>
      </Box>

      {/* Dialog hiển thị Telemetry */}
      <Dialog
        open={openDialog}
        onClose={() => setOpenDialog(false)}
        fullWidth
        maxWidth="sm"
      >
        <DialogTitle>Telemetry - {selectedDevice?.name}</DialogTitle>
        <DialogContent dividers>
          {telemetryLoading && (
            <Typography align="center" color="text.secondary">
              Đang tải dữ liệu...
            </Typography>
          )}

          {!telemetryLoading && telemetryError && (
            <Typography align="center" color="error">
              {telemetryError}
            </Typography>
          )}

          {!telemetryLoading &&
            !telemetryError &&
            (telemetry.length === 0 ? (
              <Typography align="center" color="text.secondary">
                Không có dữ liệu lịch sử.
              </Typography>
            ) : (
              <List>
                {telemetry.map((t, i) => (
                  <ListItem key={i} divider>
                    <ListItemText
                      primary={`Giá trị: ${
                        t.payload || t.value || JSON.stringify(t)
                      }`}
                      secondary={
                        t.timestamp
                          ? new Date(t.timestamp).toLocaleString()
                          : ""
                      }
                    />
                  </ListItem>
                ))}
              </List>
            ))}
        </DialogContent>
        <DialogActions>
          <Button onClick={() => setOpenDialog(false)}>Đóng</Button>
        </DialogActions>
      </Dialog>
    </Container>
  );
}

export default App;